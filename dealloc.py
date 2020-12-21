#!/usr/bin/env python3
import dlisio
import sys

def get_values(parameter):
    try:
        values = parameter.values;
        if len(values.shape)==1:
          if values.shape[0]==1:
             #print(type((parameter.values[0])))
             return parameter.values[0]
          else :
             #print(type((parameter.values)))
             return ",".join(parameter.values)
        else:
          return parameter.values
    except ValueError as e:
        sys.stderr.write(str(e)+"\n")
        sys.stderr.write(str(parameter) + "\n")
        return []

def process_parameters(parameters):
    by_long_name={p.long_name:get_values(p) for p in parameters}
    by_name={p.name:get_values(p) for p in parameters}
    combined={**by_long_name,**by_name}
    return combined

print(dlisio.__version__)
print(sys.version)
with dlisio.load("python/data/206_05a-_3_DWL_DWL_WIRE_258276498.DLIS") as d:
    for file in d:
        process_parameters(file.parameters)
