with mpi_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=188)),
 spi_mesh (trace_set_id, p_x, p_y, p_z) as (select trace_set_id, int(p_x), int(p_y), int(p_z) from proc_mesh
  where trace_set_id in (select trace_set_id from perf.trace_set where group_id=186)),
 mpi_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=188)),
 spi_tot (trace_set_id, tot) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P2_Simtick_Loop_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=186)),
 mpi_fft (trace_set_id, fft) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P3ME_ForwardFFT_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=188)),
 spi_fft (trace_set_id, fft) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='P3ME_ForwardFFT_Control' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=186)),
 mpi_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=188)),
 spi_bcast (trace_set_id, bcast) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberBcast' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=186)),
 mpi_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=188)),
 spi_reduce (trace_set_id, reduce) as
  (select trace_set_id, avg/1.0e6 from perf.performance where
    trace_label='ThrobberReduce' and trace_set_id in (select trace_set_id from perf.trace_set where group_id=186))
 select mpi_mesh.p_x*mpi_mesh.p_y*mpi_mesh.p_z, mpi_mesh.p_x, mpi_mesh.p_y, mpi_mesh.p_z, 
        mpi_tot.tot, spi_tot.tot, mpi_fft.fft, spi_fft.fft, mpi_bcast.bcast, spi_bcast.bcast, mpi_reduce.reduce, spi_reduce.reduce,
        43222.0/(mpi_mesh.p_x*mpi_mesh.p_y*mpi_mesh.p_z)
  from mpi_mesh, spi_mesh, mpi_tot, spi_tot, mpi_fft, spi_fft, mpi_bcast, spi_bcast, mpi_reduce, spi_reduce
  where mpi_tot.trace_set_id=mpi_fft.trace_set_id and mpi_tot.trace_set_id=mpi_bcast.trace_set_id
   and mpi_tot.trace_set_id=mpi_reduce.trace_set_id and spi_tot.trace_set_id=spi_fft.trace_set_id 
   and spi_tot.trace_set_id=spi_bcast.trace_set_id and spi_tot.trace_set_id=spi_reduce.trace_set_id
   and mpi_mesh.trace_set_id=mpi_tot.trace_set_id and spi_mesh.trace_set_id=spi_tot.trace_set_id
   and mpi_mesh.p_x=spi_mesh.p_x and mpi_mesh.p_y=spi_mesh.p_y and mpi_mesh.p_z=spi_mesh.p_z
       order by mpi_mesh.p_x*mpi_mesh.p_y*mpi_mesh.p_z;
