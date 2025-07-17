# Generating OpTables
To generate optables, you need to change `OPTABLES_FILTER` or `PREFIX_OPFILTERS` which contains the mapping from the opcode bit pattern to the instruction being executed (try to follow the same naming style). 

`OPTABLES_FILTER` correspond to the non-prefixed instructions, while `PREFIX_OPFILTERS` correspond to prefixed instructions.

To generate optables, just simply run `gen_optable.py`
```
python3 ./gen_optable.py
```

The script takes the template header file (`template_optable.h`), and fills in all the optable mapping defined earlier, writing it to `optable.h`.