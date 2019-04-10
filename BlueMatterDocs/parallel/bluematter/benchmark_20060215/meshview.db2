create view proc_mesh (trace_set_id, p_x, p_y, p_z) as
 with px (trace_set_id, px)
  as (select trace_set_id, substr(value, 1, 4) from perf.trace_set_data where name='p_x'),
 py (trace_set_id, py)
  as (select trace_set_id, substr(value, 1, 4) from perf.trace_set_data where name='p_y'),
 pz (trace_set_id, pz)
  as (select trace_set_id, substr(value, 1, 4) from perf.trace_set_data where name='p_z')
 select px.trace_set_id, px.px, py.py, pz.pz from px,py,pz where
  px.trace_set_id=py.trace_set_id and px.trace_set_id=pz.trace_set_id;
