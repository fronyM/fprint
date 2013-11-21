import web

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
	return render.enroll(self)	

class verify:
    def POST(self):
        return render.verify(self)

class back:
    def POST(self):
        i = db.select('fprint_user')
	return render.index(i)


if __name__ == "__main__":
    app = web.application(urls, globals())
    app.run()
