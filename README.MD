This readme is mostly just my thoughts for now. Will have full readme later
# TODO:
some basic strategies buy and sell based on two different moving averages
this means our strategies should actively share data between them and should do some calculations on them
one strategy first

lets imagine we arent aware of all changes and all we know is our start price.
our strategy will occasionally wake up after a couple of minutes, and query the exchange to get some info

I guess we will probably need to know the overall picture for backtesting and stuff maybe it would be easier if we loaded all trades since my Birthday into a file

maybe to keep it small we will truncate each one down to every 1 minute? so every minute the client will wake up and recalculate.
at system start time we save start to end of what we have done so far

oh we can also download it from here up to a certain point https://support.kraken.com/hc/en-us/articles/360047124832-Downloadable-historical-OHLCVT-Open-High-Low-Close-Volume-Trades-data

I dont think we can use the interval parameter except to get the most recent data. At that point why bother

Sounds like since is the way to go, aggregate it into sizes of 60 millis (thats what the example file does)

ideally we should eventually have the kraken client be shared between strategies and have it with locking. but for now we'll just duplicate it

## Data aggregation
What we should do is lazily load data as we need it in one minute intervals.

When loading and reading we can do both. Have our own file archive and we can read from the api when we don't see a file that exists.