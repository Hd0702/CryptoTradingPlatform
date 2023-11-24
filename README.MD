This readme is mostly just my thoughts for now. Will have full readme later
# TODO:
For data aggregation, we are going to save a local file with all OHLC data by pairs. We can break this up by pair. For now we will just focus on Kraken. 

I want to move away from coinbase and multiple exchanges to keep things simple. Having one interface for OHLC data and buys/sells is so much easier to write.

for OHLC we can have a file for every day and it should be all the minutes for that day. To start we can go 100 days back from today.

## Trades
All trades will be logged into a file with trade id and all of their data. On start we will check our open orders and which strategy is associated for it, and look to see 
if the sell point is happening right now.

## Class structure

### Data loader

### Strategy Loader

