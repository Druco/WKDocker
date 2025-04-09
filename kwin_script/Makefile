
FILES	= wkdocker/contents/code/main.js wkdocker/metadata.json
INSTALL_DIR = /usr/share/kwin/scripts

Docker.kwinscript: ${FILES}
	rm -f $@
	zip  $@ ${FILES}

install: Docker.kwinscript
	unzip -d ${INSTALL_DIR} Docker.kwinscript
