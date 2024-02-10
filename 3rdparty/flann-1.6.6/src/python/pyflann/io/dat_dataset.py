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
from scipy.io.numpyio import fwrite


def is_number(s):
    """Checks if a given string is a number.
    Parameters:
        - s (str): String to be checked.
    Returns:
        - bool: True if s is a number, False otherwise.
    Processing Logic:
        - Try to convert s to float.
        - If successful, return True.
        - If ValueError is raised, return False."""
    
    try:
        float(s)
        return True
    except ValueError:
        return False

def check(filename):
    """Checks if the given file contains a line of numbers.
    Parameters:
        - filename (str): The name of the file to be checked.
    Returns:
        - bool: True if the file contains a line of numbers, False otherwise.
    Processing Logic:
        - Open the file and read the first line.
        - If the first line starts with a '#', read the next line.
        - Split the line into a list of values.
        - If the list is empty, return False.
        - Check if all values in the list are numbers.
        - Return the result."""
    
    with open(filename,"r") as f:
        line = f.readline(5_000_000).strip()
        if line[0]=='#':  # first line might be a comment
            line = f.readline(5_000_000).strip()
    
    values = line.split()    
    if len(values)==0:
        return False
    return all(map(is_number, values))
    

def save(dataset, filename):
    """Saves a numpy array to a specified file.
    Parameters:
        - dataset (numpy.ndarray): The numpy array to be saved.
        - filename (str): The name of the file to save the array to.
    Returns:
        - None: The function does not return anything.
    Processing Logic:
        - Check if dataset is a numpy array.
        - Raise an exception if dataset is not a numpy array.
        - Save the dataset to the specified file using numpy.savetxt."""
    
    if not isinstance(dataset,numpy.ndarray):
        raise FLANNException("Can only save numpy arrays")    
    numpy.savetxt(filename,dataset, fmt="%g")

def load(filename, rows = -1, cols = -1, dtype = numpy.float32):
    """"Loads a dataset from a given file and returns it as a numpy array."
    Parameters:
        - filename (str): Name of the file to load the dataset from.
        - rows (int, optional): Number of rows to load from the dataset. Default is -1, which loads all rows.
        - cols (int, optional): Number of columns to load from the dataset. Default is -1, which loads all columns.
        - dtype (numpy.dtype, optional): Data type of the elements in the returned array. Default is numpy.float32.
    Returns:
        - dataset (numpy.ndarray): Numpy array containing the loaded dataset.
    Processing Logic:
        - Loads the dataset using numpy.loadtxt().
        - Allows for specifying the number of rows and columns to load.
        - Uses numpy.float32 as the default data type.
        - Returns the loaded dataset as a numpy array."""
    
    dataset = numpy.loadtxt(filename, dtype=dtype)
    return dataset
