/* The new stdlib additions */
open Belt;
open ServerTypes;

type state =
  | Loading
  | Error
  | Loaded(array(discordServer));

type action =
  | ServerFetch
  | ServersFetches(array(discordServer))
  | ServersFailedToFetch;



let component = ReasonReact.reducerComponent("DiscordServers");

let make = _children => {
  ...component,
  initialState: _state => Loading,
  reducer: (action, _state) =>
    switch (action) {
    | ServerFetch =>
      ReasonReact.UpdateWithSideEffects(
        Loading,
        self => {
          let _ =
            ServerAPI.getServers()
            |> Js.Promise.then_(results => {
                 self.send(ServersFetches(results));
                 Js.Promise.resolve();
               })
            |> Js.Promise.catch(err =>{
                 Js.log(err);
                 Js.Promise.resolve(self.send(ServersFailedToFetch));
               });
          ();
        },
      )
    | ServersFetches(servers) => ReasonReact.Update(Loaded(servers))
    | ServersFailedToFetch => ReasonReact.Update(Error)
    },
  didMount: self => self.send(ServerFetch),
  render: self =>
    switch (self.state) {
    | Error => <div> {ReasonReact.string("An error occurred!")} </div>
    | Loading => <div> {ReasonReact.string("Loading Severs...")} </div>
    | Loaded(servers) =>
      <div className="container">
        <h1> {ReasonReact.string("Servers")} </h1>
        {Array.map(servers, server => <DiscordServerCard server />)
         |> ReasonReact.array}
      </div>
    },
};