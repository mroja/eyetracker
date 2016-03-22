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

import sys
import json
import argparse

from predictors import create_predictor


def load_predictor(args):
    with open(args.input_file, 'r') as f:
        config = json.load(f)

    if args.selected_set:
        name = args.selected_set
    else:
        name = config['selected_set']

    if not name:
        print('')
        return None, None

    if name not in config['calibration_data']:
        print('')
        return None, None

    data = config['calibration_data'][name]
    data_preset = name.split('_')[0]

    return create_predictor(name, data), data_preset


def main(args):
    predictor, data_preset = load_predictor(args)

    if not predictor or not data_preset:
        print('')
        return

    while True:
        line = sys.stdin.readline()
        if not line:
            break

        try:
            values = list(map(float, line.split(';')))
            n_values = len(values)
        except Exception as ex:
            print(ex, file=sys.stderr)
            continue

        if n_values != 16:
            print('invalid data format (expected 16 floats, got {})'.format(n_values), file=sys.stderr)
            continue

        # pupil position on video frame
        pupil_pos = values[0:2]

        # head orientation estimate
        corners = values[2:10]
        translation = values[10:13]
        rotation = values[13:16]

        if data_preset == 'pupil':
            data = pupil_pos
        elif data_preset == 'translations':
            data = pupil_pos + translation
        elif data_preset == 'hpe':
            data = pupil_pos + translation + rotation
        elif data_preset == 'corners':
            data = pupil_pos + corners
        elif data_preset == 'all':
            data = pupil_pos + translation + rotation + corners
        else:
            continue

        try:
            prediction = predictor.predict(data)
        except Exception as ex:
            print(ex, file=sys.stderr)
            continue

        print('{:f} {:f}'.format(prediction[0], prediction[1]),
              end='\n',
              file=sys.stdout,
              flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('calibration_file', type=str,
                        help='json file with calibration data')
    parser.add_argument('-p', '--preset-name', type=str, required=False,
                        help='name of preset to load')

    parsed_args = parser.parse_args()

    main(parsed_args)
