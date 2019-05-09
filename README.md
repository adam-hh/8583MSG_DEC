#General information
A binary format message parsing program, for study and reseach.
I am only interested in 8583 messages.
If you are looking for a general network traffic capture tool, wireshark(https://www.wireshark.org/) is a perfect choice.
#extenal modules
npacp is a network traffic dump tool on windows OS.  https://github.com/nmap/npcap.git
Qt, it is a GUI library.  https://www.qt.io/ 
CMAKE(https://cmake.org/) is now the build tool 
#How to run
OS System Require Windows 64 bit, 7,8 or 10
1.install the "npcap-0.992.exe", it is a network device driver program on windows.
2.build with CMAKE tool(with CMAKE file "CMakeLists.txt").
3.run gui.exe(build from step2).
#What's news
2019/5/9 version 4.1 is released.
1. a good look GUI.
2. the captured packet lists can hold more than 1,000,000 rows.
3. automatically decode every packet when it is captured.
4. a manual decode control panel.

#What to do next
A long time task.
The decoder engine and it's interface module need to be optimized and maintained.
To support more different business protocols

#Looking for devlop partner and volunteers
Any one who are interested in 8583 message decoding.
Or just for study C/C++/Qt related development skills.
