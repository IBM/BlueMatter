--Script to create table with summary performance data for SOPE system for:
-- V4 MPI, group_id = 192
-- V4 SPI, group_id = 224
-- V5 SPI, group_id = 248
-- V5 SPI (64x64x64), group_id = 317
with mpi_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=192)),
 spi_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=224)),
 spi_v5_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=248)),
 spi_v5_64_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=317)),
 mpi_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=192)),
 spi_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=224)),
 spi_v5_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=248)),
 spi_v5_64_tot (trace_set_id, tot) as
  (select trace_set_id, avg/0.7e6 from perf.performance where
    trace_label='TimeStep:TIME:0:0' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=317)),
 mpi_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=192)),
 spi_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=224)),
 spi_v5_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=248)),
 spi_v5_64_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/0.7e6 from perf.performance where
    trace_label='PktThrobberBroadcast:TIME:0:0' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=317)),
 mpi_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=192)),
 spi_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=224)),
 spi_v5_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=248)),
 spi_v5_64_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/0.7e6 from perf.performance where
    trace_label='PktThrobberReduce:TIME:0:0' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=317))
 select mpi_mesh.p_x*mpi_mesh.p_y*mpi_mesh.p_z as node_count,
        mpi_mesh.p_x,
        mpi_mesh.p_y,
        mpi_mesh.p_z, 
        dec(round(mpi_tot.tot,2),5,2) as mpi_v4_total,
        dec(round(spi_tot.tot,2),5,2) as spi_v4_total,
        dec(round(spi_v5_tot.tot,2),5,2) as spi_v5_total,
        dec(round(spi_v5_64_tot.tot,2),5,2) as spi_v5_64_total,
        dec(round(mpi_bcast.bcast,2),5,2) as mpi_v4_bcast,
        dec(round(spi_bcast.bcast,2),5,2) as spi_v4_bcast,
        dec(round(spi_v5_bcast.bcast,2),5,2) as spi_v5_bcast,
        dec(round(spi_v5_64_bcast.bcast,2),5,2) as spi_v5_64_bcast,
        dec(round(mpi_reduce.reduce,2),5,2) as mpi_v4_reduce,
        dec(round(spi_reduce.reduce,2),5,2) as spi_v4_reduce,
        dec(round(spi_v5_reduce.reduce,2),5,2) as spi_v5_reduce,
        dec(round(spi_v5_64_reduce.reduce,2),5,2) as spi_v5_64_reduce,
        dec(round(13758.0/(mpi_mesh.p_x*mpi_mesh.p_y*mpi_mesh.p_z),1)4,1) as atoms_per_node
  from mpi_mesh,
       spi_mesh,
       spi_v5_mesh,
       spi_v5_64_mesh,
       mpi_tot, 
       spi_tot,
       spi_v5_tot,
       spi_v5_64_tot,
       mpi_bcast, 
       spi_bcast, 
       spi_v5_bcast, 
       spi_v5_64_bcast, 
       mpi_reduce, 
       spi_reduce,
       spi_v5_reduce,
       spi_v5_64_reduce
  where mpi_tot.trace_set_id=mpi_bcast.trace_set_id and 
        mpi_tot.trace_set_id=mpi_reduce.trace_set_id and 
        spi_tot.trace_set_id=spi_bcast.trace_set_id and 
        spi_tot.trace_set_id=spi_reduce.trace_set_id and 
        spi_v5_tot.trace_set_id=spi_v5_bcast.trace_set_id and 
        spi_v5_tot.trace_set_id=spi_v5_reduce.trace_set_id and 
        spi_v5_64_tot.trace_set_id=spi_v5_64_bcast.trace_set_id and 
        spi_v5_64_tot.trace_set_id=spi_v5_64_reduce.trace_set_id and 
        mpi_mesh.trace_set_id=mpi_tot.trace_set_id and 
        spi_mesh.trace_set_id=spi_tot.trace_set_id and 
        spi_v5_mesh.trace_set_id=spi_v5_tot.trace_set_id and 
        spi_v5_64_mesh.trace_set_id=spi_v5_64_tot.trace_set_id and 
        mpi_mesh.p_x=spi_mesh.p_x and 
        mpi_mesh.p_y=spi_mesh.p_y and 
        mpi_mesh.p_z=spi_mesh.p_z and
        mpi_mesh.p_x=spi_v5_mesh.p_x and 
        mpi_mesh.p_y=spi_v5_mesh.p_y and 
        mpi_mesh.p_z=spi_v5_mesh.p_z and
        mpi_mesh.p_x=spi_v5_64_mesh.p_x and 
        mpi_mesh.p_y=spi_v5_64_mesh.p_y and 
        mpi_mesh.p_z=spi_v5_64_mesh.p_z
  order by mpi_mesh.p_x*mpi_mesh.p_y*mpi_mesh.p_z;
