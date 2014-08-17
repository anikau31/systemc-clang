#!/bin/bash

emacs --batch $@ --eval "(indent-region (point-min) (point-max) nil)" -f save-buffer


