% PN Sequence Generator

% Prompt the user to enter the PN sequence type
PNSeqType = input("Enter the PN sequence type (23, 15, or 11): ");

% Define the PN polynomial based on the user's input
switch PNSeqType
    case 23
        PNpynomial =  [23 15 0];
        Scramblerpnomial = [0 -15 -23];
        PNInitialCondition = ones(23, 1);
    case 15
        PNpynomial =  [15 14 0];
        Scramblerpnomial = [0 -14 -15];
        PNInitialCondition = ones(15, 1);
    case 11
        PNpynomial =  [11 2 0];
        Scramblerpnomial = [0 -2 -11];
        PNInitialCondition = ones(11, 1);
    otherwise
        error('Invalid PN sequence type. Please enter 23, 15, or 11.');
end

% Create the PN sequence generator 
PNSeq = comm.PNSequence("Polynomial", PNpynomial ,"SamplesPerFrame",1024, "InitialConditions",PNInitialCondition);
InPutStream = PNSeq();
Scrambler = comm.Scrambler(2, Scramblerpnomial , PNInitialCondition);
descrambler = comm.Descrambler(2,Scramblerpnomial , PNInitialCondition);
ScrambledOut = Scrambler(InPutStream);
DeScrambled = descrambler(ScrambledOut);
ScramblerTest = isequal(InPutStream,DeScrambled);
modulator = comm.QPSKModulator("BitInput",true);
ModulatedSignal = modulator(ScrambledOut);
%ModulatedSignal = (ScrambledOut,4);
ModulatedSignalifft = ifft(ModulatedSignal);
scatterplot(ModulatedSignal,4);
ModulatedSignalAfterChannel= awgn(ModulatedSignalifft,15);
fftSignal = fft(ModulatedSignalAfterChannel);
scatterplot(fftSignal,4);
Demodulator = comm.QPSKDemodulator("BitOutput",true);
DemodulatedSignal = Demodulator(fftSignal);
DeScrambledRecieved = descrambler(DemodulatedSignal);


[number, ratio] = biterr(InPutStream, DeScrambledRecieved);

