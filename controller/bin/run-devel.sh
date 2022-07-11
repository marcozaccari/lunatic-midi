#!/bin/bash

ssh pi@lunatic "cd /opt/lunatic && ./lunatic-midi -debug -config-file=bin/linear_keyboard.json"
