import serial
import time
import MySQLdb as mdb
from time import gmtime, strftime

con = mdb.connect('localhost', 'datalogger', 'logger', 'weatherparams');

with con:
	# hora
	t = int(time.time())
	cur = con.cursor()
	cur.execute("CREATE TABLE IF NOT EXISTS Log(temp FLOAT, Humi FLOAT, press FLOAT, rain INT, lux FLOAT, time INT)")	
	cur.execute("DELETE FROM Log where time < ((%s)-(60*60*24))", (t))
	ser = serial.Serial('/dev/ttyACM0', 9600)
	print("Estabelecendo conexao com o Arduino...")
	time.sleep(2)
	ser.write('0')
	time.sleep(1)
	if(ser.readline()):     
		print("\nConexao estabelecida! Inicializando...")
	time.sleep(1)      

	# temperatura
	ser.write('1')
	temp = float(ser.readline())
	# umidade
	ser.write('2')
	humi = float(ser.readline())
	# pressao atm
	ser.write('3')
	press = float(ser.readline())
	# chuva
	ser.write('4')
	rain = 50*int(ser.readline())
	# lux
	ser.write('5')
	lux = int(ser.readline())
	# Disconnect
	ser.write('6')

	print(temp, humi, press, rain, lux, t)

	cur.execute("INSERT INTO Log (temp, humi, press, rain, lux, time) VALUES (%s, %s,%s,%s,%s,%s) ", (temp,humi,press,rain,lux,t))

if con:    
        con.close()
	print("Conexao encerrada...")
