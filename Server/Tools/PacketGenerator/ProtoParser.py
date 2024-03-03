
class ProtoParser():
	def __init__(self, start_id, recv_prefix, send_prefix):
		self.recv_pkt = []	# 수신 패킷 목록
		self.send_pkt = [] # 송신 패킷 목록
		self.total_pkt = [] # 모든 패킷 목록
		self.start_id = start_id # 대입
		self.id = start_id # 대입
		self.recv_prefix = recv_prefix # C_
		self.send_prefix = send_prefix # S_

	def parse_proto(self, path):
		f = open(path, 'r')
		lines = f.readlines()

		for line in lines:
			if line.startswith('message') == False: # message로 시작하는지?
				continue

			pkt_name = line.split()[1].upper() # 예시 : C_TEST 뽑아냄
			if pkt_name.startswith(self.recv_prefix):
				self.recv_pkt.append(Packet(pkt_name, self.id)) #recv_pkt에 push_back
			elif pkt_name.startswith(self.send_prefix):
				self.send_pkt.append(Packet(pkt_name, self.id)) #send_pkt에..
			else:
				continue

			self.total_pkt.append(Packet(pkt_name, self.id)) # 모든 패킷
			self.id += 1 # ++

		f.close()

class Packet:
	def __init__(self, name, id):
		self.name = name
		self.id = id