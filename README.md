# Concurrent Queues Implementation

## Table of Contents

- [Concurrent Queues Implementation](#concurrent-queues-implementation)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Project Overview](#project-overview)
  - [Getting Started](#getting-started)
  - [Usage and Installation](#usage-and-installation)


## Introduction

This project focuses on implementing concurrent queues, exploring different synchronization strategies to improve performance and thread safety. The project involves creating various queue variants and assessing their efficiency under concurrent workloads.

## Project Overview

In this project, you will find implementations for different types of concurrent queues:

1. **One lock queue**
2. **Two lock queue**
3. **Non-Blocking queue**: Michael-Scott Queue [Link to paper](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf)
4. **One lock blocking queue**

## Getting Started

1. **Queues' Code**: The queues' code and necessary interfaces are in the `queues/` directory.

2. **Driver Programs**: There are two driver programs provided for testing:
   - `driver_correctness.cpp`: Validates correctness of queue implementations.
   - `driver_throughput.cpp`: Measures the throughput of the queues.

## Usage and Installation

Follow these steps to test the different queue implementations:

1. **To Build**: Run `make` to build the project and driver programs.

2. **Correctness Testing**: To test the correctness of a queue implementation, use the format:

```bash 
$ make <queue_name>_correctness
$ ./<queue_name>_correctness --n_producers <num_producers> --n_consumers <num_consumers> --inputFile <input_file_path>
```
3. **Throughput Testing**: To measure the throughput of a queue implementation, use the format:
```bash
$ make <queue_name>_throughput
$ ./<queue_name>_throughput --n_producers <num_producers> --n_consumers <num_consumers> --seconds <duration_seconds> --init_allocator <init_allocator_value>
```


