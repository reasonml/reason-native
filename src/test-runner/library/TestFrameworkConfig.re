type requiredConfiguration = {
  snapshotDir: string,
  projectDir: string,
};

module TestFrameworkConfig = {
  type t = requiredConfiguration;
  let initialize: requiredConfiguration => t = config => config;
};
