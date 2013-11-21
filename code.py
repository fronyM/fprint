#!/bin/sh

import web
import subprocess

db = web.database(host='192.168.18.128', dbn='mysql', user='root', pw='123456', db='fprint')

render = web.template.render('templates/')

urls = (
    '/', 'index',
    '/enroll', 'enroll',
    '/verify', 'verify',
    '/back', 'back'
)

class index:
    def GET(self):
	i = db.select('fprint_user')
	return render.index(i)	

class enroll:
    def POST(self):
	info = subprocess.check_output("./enroll")
	return render.enroll(info)	

class verify:
    def POST(self):
	info = subprocess.check_output("./verify")
        return render.verify(info)

class back:
    def POST(self):
        i = db.select('fprint_user')
	return render.index(i)


if __name__ == "__main__":
    app = web.application(urls, globals())
    app.run()
