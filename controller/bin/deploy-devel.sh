#!/bin/bash

check_errors() {
	if [ ! $? -eq 0 ]; then
		echo "Error, see above"
		exit 1
	fi
}

HIGH='\033[0;31m'
NC='\033[0m'

echo -e "$HIGH Sync sources local => remote $NC\n"
rsync -av --delete --exclude=.vscode ../ pi@lunatic:/opt/lunatic
check_errors

echo -e "\n$HIGH Compile $NC\n"
ssh pi@lunatic ". /etc/profile && cd /opt/lunatic && go build"
check_errors

. run-devel.sh
