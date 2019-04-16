import dash
import pandas as pd
import plotly.graph_objs as go
import dash.dependencies as ddp
import dash_core_components as dcc
import dash_html_components as html

def plot_figure(data):
    layout = dict(
    title="Figure w/ plotly",
    )

    fig = dict(data=data, layout=layout)
    return fig

def serve_layout():
    return html.Div(children=[
        html.Div([
            html.H1("Plotly test with Live update",
                    style={"font-family": "Helvetica",
                           "border-bottom": "1px #000000 solid"}),
            ], className='banner'),
        html.Div([dcc.Graph(id='plot')],),
        dcc.Interval(id='live-update', interval=interval),
],)

second = 1000
minute = 60
hour   = 60
day    = 24
interval = 1/2*day*hour*minute*second

app = dash.Dash()

app.layout = serve_layout

def gen_plot():
    ind = ['a', 'b', 'c', 'd']
    df = pd.DataFrame({'one' : pd.Series([4., 3., 2., 1.], index=ind),
                       'two' : pd.Series([1., 2., 3., 4.], index=ind)})

    trace = [go.Scatter(x=df.index, y=df['one'])]
    fig   = plot_figure(trace)
    return fig

app.callback(
    ddp.Output('plot', 'figure'),
    [],
    [],
    [ddp.Event('live-update', 'interval')])(gen_plot)


if __name__ == '__main__':
    app.run_server(debug=True)
