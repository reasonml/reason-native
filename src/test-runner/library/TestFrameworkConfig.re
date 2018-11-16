type requiredConfiguration = {
  updateSnapshotsFlag: string,
  snapshotDir: string,
  projectDir: string,
};

module TestFrameworkConfig = {
  type t = requiredConfiguration;
  let initialize: requiredConfiguration => t = config => config;
  let getSnapshotDir: t => string = config => config.snapshotDir;
  let getUpdateSnapshotsFlag: t => string =
    config => config.updateSnapshotsFlag;
};
