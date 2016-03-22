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
import numpy as np
from predictors import PolynomialPredictor, PLSPredictor

if __name__ == '__main__':
    if len(sys.argv) <= 1:
        print('Usage: python3 predict.py calibration_file')

    config_file = sys.argv[1]
    with open(config_file, 'r') as f:
        config = json.load(f)

    use_corners = True if config['input_type'] == 'corners' else False
    
    if config['type'] == 'polynomial':
        predictor = PolynomialPredictor()
        predictor = 
    elif config['type'] == 'pls':
        predictor = PLSPredictor()
    else:
        pass


    while True:
        line = sys.stdin.readline()
        if not line:
           break

        try:
            data = map(float, line.split(';'))
            n = len(data)
        except Exception as ex:
            print(ex, file=sys.stderr)
            continue

        if n != 666:
            print('invalid data format (expected X floats, got {})'.format(n), file=sys.stderr)
            continue

        if use_corners:
            data = data[:]
        else:
            data = data[:]
            
        try:
            prediction = predictor.predict(data)
        except Exception as ex:
            print(ex, file=sys.stderr)
            continue
            
        print('{:f} {:f}\n'.format(prediction[0], prediction[1]),
              end='\n', 
              file=sys.stdout, 
              flush=True)

