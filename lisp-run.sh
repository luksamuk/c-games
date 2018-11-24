#!/bin/bash
sbcl --noinform --load game-of-life.lisp --eval "(run-game-of-life)" --quit --no-linedit
