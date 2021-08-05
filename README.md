# LoadTestingUtility
### Usage
```
./LoadTestingUtility https://www.sberbank.ru 10
```
```
docker run dmtz/load-test-utility:latest https://www.sberbank.ru 10
```
### Limitations
Currently unittests depends on network. For more stability testing need to replace real libcurl calls to mock objects.
