--Script to export table with summary performance data for Beta-Hairpin system for:
-- V4 SPI, group_id = 226
-- V5 SPI, group_id = 245
export to hairpin_table.del of del
 with spi_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=226)),
 spi_v5_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=245)),
 spi_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=226)),
 spi_v5_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=245)),
 spi_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=226)),
 spi_v5_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=245)),
 spi_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=226)),
 spi_v5_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=245))
 select spi_mesh.p_x*spi_mesh.p_y*spi_mesh.p_z as node_count,
        spi_mesh.p_x,
        spi_mesh.p_y,
        spi_mesh.p_z, 
        dec(round(spi_tot.tot,2),5,2) as spi_v4_total,
        dec(round(spi_v5_tot.tot,2),5,2) as spi_v5_total,
        dec(round(spi_bcast.bcast,2),5,2) as spi_v4_bcast,
        dec(round(spi_v5_bcast.bcast,2),5,2) as spi_v5_bcast,
        dec(round(spi_reduce.reduce,2),5,2) as spi_v4_reduce,
        dec(round(spi_v5_reduce.reduce,2),5,2) as spi_v5_reduce,
        dec(round(5239.0/(spi_mesh.p_x*spi_mesh.p_y*spi_mesh.p_z),1),4,1) as atoms_per_node
  from spi_mesh,
       spi_v5_mesh,
       spi_tot,
       spi_v5_tot,
       spi_bcast, 
       spi_v5_bcast, 
       spi_reduce,
       spi_v5_reduce
  where spi_tot.trace_set_id=spi_bcast.trace_set_id and 
        spi_tot.trace_set_id=spi_reduce.trace_set_id and 
        spi_v5_tot.trace_set_id=spi_v5_bcast.trace_set_id and 
        spi_v5_tot.trace_set_id=spi_v5_reduce.trace_set_id and 
        spi_mesh.trace_set_id=spi_tot.trace_set_id and 
        spi_v5_mesh.trace_set_id=spi_v5_tot.trace_set_id and 
        spi_mesh.p_x=spi_v5_mesh.p_x and 
        spi_mesh.p_y=spi_v5_mesh.p_y and 
        spi_mesh.p_z=spi_v5_mesh.p_z
  order by spi_mesh.p_x*spi_mesh.p_y*spi_mesh.p_z;
