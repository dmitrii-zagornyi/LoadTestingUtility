FROM ubuntu:latest

RUN apt-get update
RUN apt-get install -y libcurl4-openssl-dev
COPY bin/LoadTestingUtility /LoadTestingUtility

ENTRYPOINT ["/LoadTestingUtility"]
