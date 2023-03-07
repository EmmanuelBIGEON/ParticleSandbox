
var PSModule = {};

const ModuleInit = createParticleSandbox(PSModule);
ModuleInit.then(function(Module) {
  var aCanvas = document.getElementById('canvas');
  var aGlCtx = aCanvas.getContext ('webgl2',  { alpha: false, depth: true, antialias: true, preserveDrawingBuffer: false, stencil: true } );
  Module.canvas = aCanvas;
});
