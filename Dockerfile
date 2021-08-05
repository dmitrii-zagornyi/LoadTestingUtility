FROM ubuntu:latest

RUN pwd
COPY bin/LoadTestingUtility /LoadTestingUtility
RUN apt-get update
RUN apt-get install -y libcurl4-openssl-dev

ENTRYPOINT ["/LoadTestingUtility"]
