Assignment

Solution file: Assignment.sln

Known Issues: 
Leaking D3D memory somewhere. Most likely leaking from Game.cpp because it is a mess and not engine side.
Removed Memory Tracking and Shutting Down the Logger because they broke stuff and I had enough of it!

How To Use:
Press
  0 = Play one off effect       Note: Uses the space effect so look the same as one of the Persistent effect emitters. Recommend Destroying the Persistent Effect before playing.
  1 = Destroys the persistent effect, but lets it play out
  2 = Destroys the persistent effect immediately

 Particle System Code contains lots of comments to explain my thought process and how to use the system.
 SetupParticleStuff in Game.cpp shows an example of creating an emitter from code and adding it to a ParticleSystem that was built from XML code.

Commands:
profile_log_report 	| No params. Prints out the report to the logger. Call log_flush afterwards to hopefully avoid above issues stopping the write
profile_pause 		| No params. Pauses the profiler.
profile_resume 		| No params. Resumes profiling.
profile_view_flat	| No params. Changes the profiler to print/render with a flat view
profile_view_tree	| No params. Changes the profiler to print/render with a tree view

log_copy 		| Params: filepath. Closes log, makes a copy (not sure if it works right, it just made an empty file but my log may have been empty) and  


Code is under the ParticlesSystem filter. Includes: ParticleManager, ParticleSystem, ParticleEmitter, and Particle.
See a05_particles.docx to see what was attempted.