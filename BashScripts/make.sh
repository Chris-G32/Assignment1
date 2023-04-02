#!/bin/bash
g++ consumer.cpp -g -pthread -lrt -o consumer
g++ producer.cpp -g -pthread -lrt -o producer