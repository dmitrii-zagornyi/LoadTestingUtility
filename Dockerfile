FROM ubuntu:latest

COPY bin/LoadTestingUtility /LoadTestingUtility

ENTRYPOINT ["/LoadTestingUtility"]