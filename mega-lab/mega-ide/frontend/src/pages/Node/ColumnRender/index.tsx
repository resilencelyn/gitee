import { Fragment } from "react";

export default (props: {
  main: string;
  sub?: string;
}) => {
  const { main, sub = '' } = props;

  return (
    <Fragment>
      <div style={{ fontWeight: 700, color: '#242e42' }}>
        {main}
      </div>
      <div style={{ fontWeight: 400, color: '#79879c' }}>
        {sub}
      </div>
    </Fragment>
  )
}

export { default as PercentRender } from './PercentRender';
export { default as IORender } from './IORender';