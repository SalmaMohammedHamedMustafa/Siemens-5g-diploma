clear;
main();
function [number, ratio] = pn_sequence_modulation(modType, PNSeqType, SamplesPerFrame, SNR)

    % Define the PN polynomial and initial conditions based on the user's input
    switch PNSeqType
        case 23
            PNpynomial = [23 15 0];
            Scramblerpnomial = [0 -15 -23];
            PNInitialCondition = ones(23, 1);
        case 15
            PNpynomial = [15 14 0];
            Scramblerpnomial = [0 -14 -15];
            PNInitialCondition = ones(15, 1);
        case 11
            PNpynomial = [11 2 0];
            Scramblerpnomial = [0 -2 -11];
            PNInitialCondition = ones(11, 1);
        otherwise
            error('Invalid PN sequence type. Please enter 23, 15, or 11.');
    end

    % Create the PN sequence generator
    PNSeq = comm.PNSequence("Polynomial", PNpynomial, "SamplesPerFrame", SamplesPerFrame, "InitialConditions", PNInitialCondition);
    InPutStream = PNSeq();
    Scrambler = comm.Scrambler(2, Scramblerpnomial, PNInitialCondition);
    descrambler = comm.Descrambler(2, Scramblerpnomial, PNInitialCondition);
    ScrambledOut = Scrambler(InPutStream);
    
    % Modulation
    switch modType
        case 'BPSK'
            M = 2; % BPSK
            modulator = @(x) pskmod(x, M);
            Demodulator = @(x) pskdemod(x, M);
        case 'QPSK'
            M = 4; % QPSK (which is 4-PSK)
            modulator = @(x) pskmod(x, M, pi/4, 'InputType', 'bit');
            Demodulator = @(x) pskdemod(x, M, pi/4, 'OutputType', 'bit');
        case '16QAM'
            M = 16; % 16-QAMtrue);
              Demodulator = @(x) qamdemod(x, M, 'OutputType', 'bit', 'UnitAveragePower', true);
            modulator = @(x) qammod(x, M, 'InputType', 'bit', 'UnitAveragePower', true);
        % Add more cases for other modulation types as needed
        otherwise
            error('Invalid modulation type. Please use "BPSK", "QPSK", or "16QAM".');
    end
    
    ModulatedSignal = modulator(ScrambledOut);
    ModulatedSignalifft = ifft(ModulatedSignal);
    scatterplot(ModulatedSignal,4);
    
    % Channel (AWGN)
    ModulatedSignalAfterChannel = awgn(ModulatedSignalifft, SNR);
    
    % Demodulation
    fftSignal = fft(ModulatedSignalAfterChannel);
    scatterplot(fftSignal,4);
    DemodulatedSignal = Demodulator(fftSignal);
    DeScrambledRecieved = descrambler(DemodulatedSignal);

    % Calculate bit error rate
    [number, ratio] = biterr(InPutStream, DeScrambledRecieved);

end

function main()
    while true
        % Prompt the user for the modulation type
        modType = input('Enter the modulation type (BPSK, QPSK, 16QAM): ', 's');

        % Prompt the user for the PN sequence type
        PNSeqType = input('Enter the PN sequence type (23, 15, 11): ');

        % Prompt the user for the Samples per frame
        SamplesPerFrameNum = input('Enter the Samples per frame (divisible by 2): ');

        % Validate the inputs
        validModTypes = {'BPSK', 'QPSK', '16QAM'};
        if ~ismember(modType, validModTypes)
            error('Invalid modulation type. Please enter "BPSK", "QPSK", or "16QAM".');
        end

        validPNSeqTypes = [23, 15, 11];
        if ~ismember(PNSeqType, validPNSeqTypes)
            error('Invalid PN sequence type. Please enter 23, 15, or 11.');
        end

        % Initialize arrays to store SNR and BER values
        snrValues = 4:15;
        berValues = zeros(size(snrValues));

        % Run the pn_sequence_modulation function for each SNR value
        for i = 1:length(snrValues)
            [number, ratio] = pn_sequence_modulation(modType, PNSeqType, SamplesPerFrameNum, snrValues(i));
            berValues(i) = ratio;
        end

        % Plot the results
        figure;
        plot(snrValues, berValues, '-o');
        title('BER vs. SNR');
        xlabel('SNR (dB)');
        ylabel('Bit Error Rate (BER)');
        grid on;

        % Ask the user if they want to continue
        choice = input('Do you want to run another simulation? (y/n): ', 's');
        if lower(choice) ~= 'y'
            break;
        end
    end
end