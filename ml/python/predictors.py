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

import pickle
import base64

import numpy as np

from sklearn.preprocessing import PolynomialFeatures
from sklearn.linear_model import LinearRegression
from sklearn.cross_decomposition import PLSRegression

# http://stats.stackexchange.com/questions/4517/regression-with-multiple-dependent-variables


class PolynomialPredictor:
    def __init__(self):
        self._create_model(2)

    def predict(self, measured_values):
        values_poly = self.polynomial_features.fit_transform(measured_values)
        # print(values_poly)

        predicted_x = self.linear_regression_x.predict(values_poly)
        predicted_y = self.linear_regression_y.predict(values_poly)
        # print(predicted_x, predicted_y)

        return predicted_x[0], predicted_y[0]

    def train(self, measured_values, screen_points, degree=2):
        assert measured_values.shape[0] == screen_points.shape[0]
        # print('measured_values:\n{}'.format(measured_values))
        # print('screen_points:\n{}'.format(screen_points))

        self._create_model(degree)

        screen_points_x = screen_points[:, 0]
        screen_points_y = screen_points[:, 1]

        measured_values_poly = self.polynomial_features.fit_transform(measured_values)

        self.linear_regression_x.fit(measured_values_poly, screen_points_x)
        self.linear_regression_y.fit(measured_values_poly, screen_points_y)

    def save(self):
        return base64.a85encode(pickle.dumps({
            'polynomial_features': self.polynomial_features,
            'linear_regression_x': self.linear_regression_x,
            'linear_regression_y': self.linear_regression_y
        })).decode('ascii')

    def load(self, data):
        obj = pickle.loads(base64.a85decode(data.encode('ascii')))
        self.polynomial_features = obj['polynomial_features']
        self.linear_regression_x = obj['linear_regression_x']
        self.linear_regression_y = obj['linear_regression_y']

    def _create_model(self, degree):
        self.polynomial_features = \
            PolynomialFeatures(degree=degree,
                               interaction_only=False,
                               include_bias=True)
        self.linear_regression_x = \
            LinearRegression(fit_intercept=True,
                             normalize=False,
                             copy_X=True,
                             n_jobs=-1)
        self.linear_regression_y = \
            LinearRegression(fit_intercept=True,
                             normalize=False,
                             copy_X=True,
                             n_jobs=-1)


class PLSPredictor:
    def __init__(self):
        self._create_model()

    def predict(self, values):
        return self.pls.predict(values)[0]

    def train(self, measured_values, screen_points):
        self._create_model()
        self.pls.fit(measured_values, screen_points)

    def save(self):
        return base64.a85encode(pickle.dumps({
            'pls': self.pls
        })).decode('ascii')

    def load(self, data):
        # data is expected to be str
        obj = pickle.loads(base64.a85decode(data.encode('ascii')))
        self.pls = obj['pls']

    def _create_model(self):
        self.pls = PLSRegression(n_components=2,
                                 scale=True,
                                 max_iter=500,
                                 tol=1e-06,
                                 copy=True)


def create_predictor(name, data):
    if 'poly' in name:
        predictor = PolynomialPredictor()
    else:
        predictor = PLSPredictor()
    predictor.load(data)
    return predictor


def _test():
    measured_values = np.array([[0, 0, 0.5, 2],
                                [0, 1, 0.5, 2],
                                [1, 0, 0.5, 2],
                                [1, 1, 0.5, 2]])

    screen_points = np.array([[0, 0],
                              [0, 1],
                              [1, 0],
                              [1, 1]])

    prediction_point = np.array([0.5, 0.5, 0.5, 0.5])

    poly_predictor = PolynomialPredictor()
    poly_predictor.train(measured_values, screen_points, degree=2)
    poly_predictor_serialized = poly_predictor.save()
    poly_predictor.load(poly_predictor_serialized)
    poly_prediction = poly_predictor.predict(prediction_point)
    print(poly_prediction)

    pls_predictor = PLSPredictor()
    pls_predictor.train(measured_values, screen_points)
    pls_predictor_serialized = poly_predictor.save()
    pls_predictor.load(pls_predictor_serialized)
    pls_prediction = pls_predictor.predict(prediction_point)
    print(pls_prediction)

if __name__ == '__main__':
    _test()
