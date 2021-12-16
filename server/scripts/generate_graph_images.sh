#!/bin/sh -e
ls | grep -F depsgraph.dot | xargs -I {} dot -Tpng -o {}.png {}
