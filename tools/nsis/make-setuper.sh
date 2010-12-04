#!/bin/sh

# Convert file to cp1251
iconv --from=utf8 --to=cp1251 basketpwd-utf8.nsi > basketpwd-cp1251.nsi

makensis basketpwd-cp1251.nsi
