import sys, json
from collections import OrderedDict

lines = [ line.strip() for line in sys.stdin if line.strip() ]

by_benchtype = {}

for line in lines:
    benchtype, nkeys, program, load_factor, nbytes, runtime = line.split(',')
    nkeys = int(nkeys)
    nbytes = int(nbytes)
    runtime = float(runtime)
    load_factor = float(load_factor)

    by_benchtype.setdefault("%s_runtime" % benchtype, {}).setdefault(program, []).append([nkeys, runtime, load_factor])
    if benchtype in ('insert_random_full', 'insert_small_string', 'insert_string',
                     'insert_random_full_reserve', 'insert_small_string_reserve', 'insert_string_reserve'):
        by_benchtype.setdefault("%s_memory"  % benchtype, {}).setdefault(program, []).append([nkeys, nbytes, load_factor])

proper_names = OrderedDict([
    ('std_map', 'std::map'),
    ('std_flat_map', 'std::flat_map'),
    ('std_unordered_map', 'std::unordered_map'),
    ('boost_map', 'boost::containers::unordered_map'),
    ('boost_flat_map', 'boost::containers::flat_map'),
    ('boost_unordered_map', 'boost::unordered_map'),
    # ('etl_flat_map', 'etl::flat_map')
])

# do them in the desired order to make the legend not overlap the chart data
# too much
program_slugs = [
    'std_map',
    'std_flat_map',
    'std_unordered_map',
    'boost_map',
    'boost_flat_map',
    'boost_unordered_map',
    'loki_assoc_vector',
    'etl_flat_map"'
]

# hashmap which will be shown (checkbox checked)
default_programs_show = [
    'std_map',
    'std_flat_map',
    'std_unordered_map',
    'boost_map',
    'boost_flat_map',
    'boost_unordered_map',
    'loki_assoc_vector',
    # 'etl_flat_map'
]

chart_data = {}

for i, (benchtype, programs) in enumerate(by_benchtype.items()):
    chart_data[benchtype] = []
    for j, program in enumerate(program_slugs):
        if program not in programs:
            continue
        
        data = programs[program]
        chart_data[benchtype].append({
            'program': program,
            'label': proper_names[program],
            'data': [],
        })

        for k, (nkeys, value, load_factor) in enumerate(data):
            chart_data[benchtype][-1]['data'].append([nkeys, value, load_factor])

json_text = json.dumps(chart_data)
json_text = json_text.replace("}], ", "}], \n")
print('chart_data = ' + json_text + ';')
print('\nprograms = ' + json.dumps(proper_names, indent=1) + ';')
print('\ndefault_programs_show = ' + str(default_programs_show) + ';')
