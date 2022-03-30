#coding=utf-8
import sys,datetime,time,os,os.path,stat,re

repattern = re.compile(r'.*[\.]{1}([^\.]+)')

def changeFileTime(path, ctime):
    for parent,dirnames,filenames in os.walk(path):
        for dirname in  dirnames:
            changeFileTime(os.path.join(parent,dirname), ctime)
        for filename in filenames:
            match = repattern.match(filename)
            if match:
                filetype = match.groups()[0]
                if filetype in ['h', 'c', 'cc', 'cpp', 'hpp', 'inl']:
                    name = os.path.join(parent,filename)
                    st = os.stat(name)
                    timestamp = st[stat.ST_MTIME]
                    if timestamp > ctime:
                        date = datetime.datetime.fromtimestamp(timestamp)
                        os.utime(name,(st[stat.ST_ATIME], ctime))
                        print name,'modify time: ',date.strftime('%Y-%m-%d %H:%M:%S')


if __name__ == '__main__':
    if len(sys.argv) <= 1:
        print "Useage: chgFileTime path"
        sys.exit(0)
    changeFileTime(sys.argv[1], time.time())
