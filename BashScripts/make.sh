#!/bin/bash
g++ consumer.cpp -pthread -lrt -o consumer
g++ producer.cpp -pthread -lrt -o producer