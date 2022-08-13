declare module "*.wasm" {
  const content: string;
  export default content;
}

declare module "*.ppn" {
  const content: string;
  export default content;
}

declare module 'web-worker:*' {
  const WorkerFactory: new () => Worker;
  export default WorkerFactory;
}
