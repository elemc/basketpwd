TEMPLATE = subdirs
SUBDIRS = mainapp library
mainapp.file = basketpwd-gui.pro
mainapp.depends = library
library.subdir = libs/basketpwd

