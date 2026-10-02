import re
import os

srs = re.compile('.*\[\$SRS_SPM (\d+)\s?\].*')

req = []

with open('./doc/Requirements/10_Requirements.rst', 'r', encoding='iso-8859-1') as fr :
    for line in fr.readlines() :
        if srs.match(line) :
            req.append(srs.match(line).group(1))

req = sorted(req, reverse=True)

print(f"SRS: {req[0]}")

dds = re.compile('.*\[\$DDS_SPM (\d+)\s?\].*')

req = []

f_md = [os.path.join('./doc/DesignNotes', f) for f in os.listdir('./doc/DesignNotes') if os.path.isfile(os.path.join('./doc/DesignNotes', f))]

for f in f_md:
    with open(f, 'r', encoding='iso-8859-1') as fr :
        for line in fr.readlines() :
            if dds.match(line) :
                req.append(dds.match(line).group(1))

req = sorted(req, reverse=True)

print(f"DDS : {req[0]}")