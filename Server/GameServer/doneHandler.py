#coding=utf-8
import os,os.path

def isFileHandler(path):
	iflie = open(path, 'r')
	if iflie == None:
		return  False
	lines = iflie.readlines()
	iflie.close()
	for line in lines:
		if line.find('this_source_type') >= 0:
			return True
	return False

def analysisLine(line):
	key = ''
	info = []
	index = line.find('this_process_handle_')
	if index >= 0:
		key = line[index + 20: line.find("(")]
	index = line.find('//')
	if index >= 0:
		info.append(line[index+2:])
	info.append(line)

def SaveHandlers(handlers, path):
	oflie = open(path[0], 'w')
	if oflie == None:
		return
	for key in sorted(handlers.keys()):
		val = handlers[key]
		oflie.write(val[0] + '\n');
	oflie.close()

	oflie = open(path[1], 'w')
	if oflie == None:
		return
	#oflie.write('#include \"MG_TYPES.h\"\n')
	#oflie.write('#include \"HL_PacketHandler.h\"\n')
	#oflie.write('#include \"hl_handler.h\"\n')
	#oflie.write('using namespace Protocol;\n')
	#oflie.write('void g_registerGameHandler()\n{\n')
	for key in sorted(handlers.keys()):
		val = handlers[key]
		oflie.write( '\tregister_handler(%s, %s, %s, (handler_func_type)this_process_handle_%s, __FILE__, __LINE__);\n'%(val[1].strip(), val[2].strip(), val[3].strip(), key));
	#oflie.write('}\n')
	oflie.close()

def AddHandler(handlers, path):
	iflie = open(path, 'r')
	if iflie == None:
		return
	lines = iflie.readlines()
	iflie.close()
	this_source_type = ""
	for line in lines:
		line = line.replace("\n","")
		if this_source_type == "" and line.find('this_source_type') >= 0:
			this_source_type = line[line.find('this_source_type')+16:].strip()
			if this_source_type.find('//') >= 0:
				this_source_type = this_source_type[:this_source_type.find('//')].strip()

		index = line.find('ON_HANDLE(')
		indexNote = line.find('//')
		if index >= 0 and ( indexNote < 0 or indexNote > index + 9):
			indexEnd = line.find(')')
			key = line[index+10:indexEnd].strip().replace('this_source_type', this_source_type)
			args = key.split(',')
			print(args)
			node = []
			handlerKey=""
			node.append("extern void this_process_handle_%s_%s(Protocol::SS_MG_HEAD& rkMsgHead, NW_ServerSession * pkSession, GameLogic::LC_ActorBase* pkTarget, char* pMsgBody, uint16_t usBodySize);//%s"%(args[0].strip(), args[1].strip(), this_source_type))
			node.extend(args)
			handlerKey = "%s_%s"%(args[0].strip(), args[1].strip())
			handlers[handlerKey] = node
			
if __name__ == '__main__':
	localhandlers = {}
	for parent,dirnames,filenames in os.walk('Src/Handler'):
		for filename in filenames:
			strLen = len(filename)
			if strLen > 4 and filename[strLen-4:strLen] == ".cpp":
				name = os.path.join(parent,filename)
				if isFileHandler(name) == True:
					AddHandler(localhandlers, name)

	SaveHandlers(localhandlers,['Src/Handler/Hl_handler_def.h','Src/Handler/Hl_handler_reg.h'])
