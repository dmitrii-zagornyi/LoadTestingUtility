FROM ubuntu:latest

RUN sudo apt-get install -y libcurl4-openssl-dev
COPY bin/LoadTestingUtility /LoadTestingUtility

ENTRYPOINT ["/LoadTestingUtility"]
