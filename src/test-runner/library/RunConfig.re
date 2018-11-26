module RunConfig = {
  type t = {updateSnapshots: bool};

  let initialize = () => {updateSnapshots: false};

  let updateSnapshots: (bool, t) => t =
    (updateSnapshots, config) => {updateSnapshots: updateSnapshots};
};
