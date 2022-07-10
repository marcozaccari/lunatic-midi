#!/bin/bash

rsync -av --delete --exclude=.vscode ../controller/ pi@lunatic-devel:/opt/lunatic

ssh pi@lunatic-devel "cd /opt/lunatic && go build && ./main -config-file=bin/linear_keyboard.json"
