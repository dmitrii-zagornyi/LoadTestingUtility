FROM windows:latest

COPY LoadTestingUtility\x64\Release\libcurl.dll /libcurl.dll
COPY LoadTestingUtility\x64\Release\libcurl.pdb /libcurl.pdb
COPY LoadTestingUtility\x64\Release\LoadTestingUtility.exe /LoadTestingUtility.exe
COPY LoadTestingUtility\x64\Release\zlib1.dll /zlib1.dll
COPY LoadTestingUtility\x64\Release\zlib.pdb /zlib.pdb

ENTRYPOINT ["/LoadTestingUtility.exe"]