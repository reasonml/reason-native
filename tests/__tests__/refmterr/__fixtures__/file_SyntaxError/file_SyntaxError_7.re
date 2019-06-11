module Input: {
  /*
   * Components that don't accept Children, would not have parameterized
   * `element`. Child-accepting components would.
   */
  type state = string;
  type subtree;
  type tree = state => subtree;
  let createElement: (~initialVal: string=?, 'c) => React.elem(tree);
} = {
  type state = string;
  type subtree = Dom.tree(React.empty);
  type tree = state => subtree;
  let createElement =
      (~initialVal="deafult", children) => ((~state=initialVal, self) => (
    state,
    fun
    | React.Changed(s) => s,
    React.One(Dom.div(~className="helloClass", Empty)),
  ) : React.elem(tree);
};
