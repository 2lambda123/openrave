#Copyright 2008-2009  Marius Muja (mariusm@cs.ubc.ca). All rights reserved.
#Copyright 2008-2009  David G. Lowe (lowe@cs.ubc.ca). All rights reserved.
#
#THE BSD LICENSE
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions
#are met:
#
#1. Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#2. Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
#THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
#IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
#OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
#IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
#INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
#NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
#THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from __future__ import with_statement

from pyflann.exceptions import FLANNException
import numpy
import os.path

def check(filename):
    """"Checks if the first 6 characters of a file match the string 'BINARY' and returns True if they do, False otherwise."
    Parameters:
        - filename (str): The name of the file to be checked.
    Returns:
        - bool: True if the first 6 characters of the file match 'BINARY', False otherwise.
    Processing Logic:
        - Open the file in read mode.
        - Read the first 6 characters of the file.
        - Check if the first 6 characters match 'BINARY'.
        - Return True if they do, False otherwise."""
    
    f = open(filename,"r")
    header = f.read(6)
    if header[0:6]=="BINARY":
        return True
    return False

def save(dataset, filename):
    """Saves a dataset to a file.
    Parameters:
        - dataset (numpy.ndarray): Dataset to be saved.
        - filename (str): Name of the file to save the dataset to.
    Returns:
        - None: No return value.
    Processing Logic:
        - Check if dataset is in numpy format.
        - Write dataset shape and dtype to file.
        - Write dataset to file."""
    
    if not isinstance(dataset,numpy.ndarray):
        raise FLANNException("Dataset must be in numpy format")
    
    with open(filename+".meta", 'w') as fd_meta:
        fd_meta.write(\
"""BINARY
%d
%d
%s"""%(dataset.shape[0],dataset.shape[1],dataset.dtype.name))

    dataset.tofile(filename)


def load(filename, rows = -1, cols = -1, dtype = numpy.float32):
    """"Loads a dataset from a binary file and returns it as a numpy array."
    Parameters:
        - filename (str): Name of the binary file to be loaded.
        - rows (int): Number of rows in the dataset. If not specified, it will be read from the .meta file.
        - cols (int): Number of columns in the dataset. If not specified, it will be read from the .meta file.
        - dtype (numpy.dtype): Data type of the elements in the dataset. If not specified, it will be read from the .meta file.
    Returns:
        - data (numpy.ndarray): Numpy array containing the dataset.
    Processing Logic:
        - Checks if a .meta file exists for the given filename.
        - If .meta file exists, reads the header information from it.
        - If .meta file does not exist, checks if rows and cols are specified.
        - If rows and cols are not specified, raises an error.
        - Loads the dataset from the binary file using the specified data type.
        - Reshapes the loaded data into a 2D array with the specified number of rows and columns."""
    
    
    if os.path.isfile(filename+".meta"):        
        with open(filename+".meta","r") as fd:
            header = fd.readline(5_000_000)
            assert( header[0:6] == "BINARY")
            rows = int(fd.readline(5_000_000))
            cols = int(fd.readline(5_000_000))
            dtype = numpy.dtype(fd.readline(5_000_000).strip())
    else:
        if rows==-1 or cols==-1:
            raise "No .meta file present, you must specify dataset rows, cols asd dtype"
    data = numpy.fromfile(file=filename, dtype=dtype, count=rows*cols)
    data.shape = (rows,cols)
    return data
    
