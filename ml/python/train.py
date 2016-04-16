#!/usr/bin/env python3

#
# This file is part of PISAK.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# Copyright 2015, Alex Khrabrov <alex@mroja.net>
#

import json
import math
import random
import argparse
import multiprocessing

import numpy as np

from predictors import PolynomialPredictor, PLSPredictor, create_predictor


def load_calibration_data(input_file):
    with open(input_file) as f:
        data = json.load(f)
    return data['calibration_data']


def percentage_split(seq, percentages):
    cdf = np.cumsum(percentages)
    assert cdf[-1] == 1.0
    stops = list(map(int, cdf * len(seq)))
    return [seq[a:b] for a, b in zip([0] + stops, stops)]


def split_calibration_data(calibration_data, train_proportion=0.7):
    random.shuffle(calibration_data)
    return percentage_split(calibration_data, [train_proportion, 1.0 - train_proportion])


def get_data_presets_from_calibration_data(calibration_data):
    screen_points = []
    measured_values = [('pupil', []),
                       ('translations', []),
                       ('hpe', []),
                       ('corners', []),
                       ('all', [])]

    for d in calibration_data:
        screen_points.append(d[0:2])

        # pupil position on video frame
        pupil_pos = d[2:4]

        # head orientation estimates
        corners = d[4:12]
        translation = d[12:15]
        rotation = d[15:18]

        val = [
            pupil_pos,
            pupil_pos + translation,
            pupil_pos + translation + rotation,
            pupil_pos + corners,
            pupil_pos + translation + rotation + corners
        ]

        for i in range(len(val)):
            measured_values[i][1].append(val[i])

    screen_points = np.array(screen_points)
    for i in range(len(measured_values)):
        measured_values[i] = measured_values[i][0], np.array(measured_values[i][1])

    return screen_points, measured_values


def generate_configs(screen_points, measured_values):
    configs = []
    for data_preset_name, values in measured_values:
        for degree in range(2, 25):
            v = (len(values[0]) ** 1.6) * (degree ** 1.6)
            if v > 500:
                print('skipping n_data = {}, degree = {}, v = {}'.format(len(values[0]), degree, v))
                continue
            configs.append({
                'predictor_type': 'poly',
                'screen_points': screen_points,
                'measured_values': values,
                'data_preset_name': data_preset_name,
                'degree': degree
            })

        configs.append({
            'predictor_type': 'pls',
            'screen_points': screen_points,
            'measured_values': values,
            'data_preset_name': data_preset_name
        })

    print('total configurations to train:', len(configs))
    random.shuffle(configs)    
    return configs


def train_predictor_worker(config):
    print(config['data_preset_name'],
          config['predictor_type'],
          config['degree'] if config['predictor_type'] == 'poly' else '')
    if config['predictor_type'] == 'pls':
        try:
            pls_predictor = PLSPredictor()
            pls_predictor.train(config['measured_values'],
                                config['screen_points'])
            data = pls_predictor.save()
            output = {
                'name': '{}_pls'.format(config['data_preset_name']),
                'data': data
            }
            # print(pls_predictor)
            # print(data)
            return output
        except Exception as ex:
            print(ex)
    else:
        try:
            poly_predictor = PolynomialPredictor()
            poly_predictor.train(config['measured_values'],
                                 config['screen_points'],
                                 degree=config['degree'])
            data = poly_predictor.save()
            output = {
                'name': '{}_poly_{}'.format(config['data_preset_name'], config['degree']),
                'data': data
            }
            # print(poly_predictor)
            # print(data)
            return output
        except Exception as ex:
            print(ex)


def train_predictors(calibration_data):
    screen_points, measured_values = \
        get_data_presets_from_calibration_data(calibration_data)
    
    print('data loaded...')

    configs = generate_configs(screen_points, measured_values)

    print('configs created...')

    num_tasks = len(configs)
    output_data = []
    with multiprocessing.Pool() as pool:
        for i, data in enumerate(pool.imap_unordered(train_predictor_worker, configs), 1):
            print('done {:.2%}'.format(i / num_tasks))
            output_data.append(data)
        pool.close()

    serialized_predictors = {}
    for d in output_data:
        serialized_predictors[d['name']] = d['data']
    return serialized_predictors


def evaluate_predictor_worker(config):
    print(config['name'])

    input_data_type = config['name'].split('_')[0]

    predictor = create_predictor(config['name'], config['serialized_predictor'])
    
    predictions = []
    for d in config['test_data']:
        # screen_points = d[0:2]

        # pupil position on video frame
        pupil_pos = d[2:4]

        # head orientation estimates
        corners = d[4:12]
        translation = d[12:15]
        rotation = d[15:18]

        if input_data_type == 'pupil':
            input_vec = pupil_pos
        elif input_data_type == 'translations':
            input_vec = pupil_pos + translation
        elif input_data_type == 'hpe':
            input_vec = pupil_pos + translation + rotation
        elif input_data_type == 'corners':
            input_vec = pupil_pos + corners
        elif input_data_type == 'all':
            input_vec = pupil_pos + translation + rotation + corners
        else:
            raise Exception('??? {} ???'.format(input_data_type))

        predictions.append(predictor.predict(input_vec))

    sum_sq_x = 0.0
    sum_sq_y = 0.0

    for data, predicted in zip(config['test_data'], predictions):
        real = data[0:2]
        sum_sq_x += (real[0] - predicted[0]) ** 2
        sum_sq_y += (real[1] - predicted[1]) ** 2

        # print('{:.2f} {:.2f} - {:.2f} {:.2f}'.format(
        #    real[0], real[1],
        #    predicted[0], predicted[1]))

    sum_sq_x /= len(config['test_data'])
    sum_sq_y /= len(config['test_data'])

    print('{} - {:.2f} {:.2f}'.format(config['name'], sum_sq_x, sum_sq_y))

    output = {
        'name': config['name'],
        'sum_sq_x': sum_sq_x, 
        'sum_sq_y': sum_sq_y
    }
    
    return output


def evaluate_predictors(serialized_predictors, test_data):
    configs = []

    for k, v in serialized_predictors.items():
        configs.append({
            'name': k,
            'serialized_predictor': v,
            'test_data': test_data,

        })

    num_tasks = len(configs)
    output_data = []
    with multiprocessing.Pool() as pool:
        for i, data in enumerate(pool.imap_unordered(evaluate_predictor_worker, configs), 1):
            print('done {:.2%}'.format(i / num_tasks))
            output_data.append(data)
        pool.close()

    return output_data


def save_train_and_test_data(train_data, test_data, input_file):
    serialized_train_data = json.dumps({'calibration_data': train_data})
    serialized_test_data = json.dumps({'calibration_data': test_data})

    output_file_train = input_file.replace('.json', '') + '.evaluation.train.json'
    output_file_test = input_file.replace('.json', '') + '.evaluation.test.json'

    with open(output_file_train, 'w') as f:
        f.write(serialized_train_data)
    with open(output_file_test, 'w') as f:
        f.write(serialized_test_data)


def main(input_file):
    calibration_data = load_calibration_data(input_file)

    train_data, test_data = split_calibration_data(calibration_data,
                                                   train_proportion=0.7)

    save_train_and_test_data(train_data, test_data, input_file)

    serialized_predictors = train_predictors(train_data)

    predictors_score = evaluate_predictors(serialized_predictors, test_data)

    with open(input_file.replace('.json', '') + '.evaluation.json', 'w') as f:
        f.write(json.dumps(predictors_score))

    scores = []
    for score in predictors_score:
        s = math.sqrt(score['sum_sq_x']) + math.sqrt(score['sum_sq_y'])
        scores.append((score['name'], s))
    scores.sort(key=lambda s: s[1])

    for s, n in scores:
        print('{} - {}'.format(n, s))

    output = {
        'selected_predictor': scores[0][0],
        'predictors': serialized_predictors
    }

    # print(output)
    output_str = json.dumps(output)
    # print(output_str)

    output_file_name = input_file.replace('.json', '.trained.json')
    with open(output_file_name, 'w') as f:
        f.write(output_str)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('input_file', type=str,
                        help='json file with data collected during calibration')
    parsed_args = parser.parse_args()
    main(parsed_args.input_file)

