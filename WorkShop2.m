clear;
close all;
main();

function main()
    % Get user inputs
    PNSeqType = input('Enter the PN sequence type (23, 15, 11): ');
    SamplesPerFrameNum = input('Enter the Samples per frame (divisible by all bps for all modulations): ');
    operation = input('What do you want to do? (1- see BER with no impairments 2- CP 3- impairments 4- conv): ');

    switch operation
        case 1
            action = 1;
            % Calculate BER for all modulation types
            [snrValues, berResults, modTypes] = calculate_ber(PNSeqType, SamplesPerFrameNum, action);
        
            % Call the plot function
            plot_ber(snrValues, berResults, modTypes);
        case 2
            CPLength = input('Enter CP length: ');
            run_cp_for_all_modulations(PNSeqType, SamplesPerFrameNum, CPLength);
        case 3
            action = 2;
            % Calculate BER for all modulation types with impairments
            [snrValues, berResults, modTypes] = calculate_ber(PNSeqType, SamplesPerFrameNum, action);
        
            % Call the plot function
            plot_ber(snrValues, berResults, modTypes);
        case 4
            action = 3;
            % Calculate BER for all modulation types with convolutional encoding
            [snrValues, berResults, modTypes] = calculate_ber(PNSeqType, SamplesPerFrameNum, action);
        
            % Call the plot function
            plot_ber(snrValues, berResults, modTypes);
    end
end

function run_cp_for_all_modulations(PNSeqType, SamplesPerFrameNum, CPLength)
    % Define SNR
    SNR = 50;  % Example SNR value in dB
    
    % Channel impulse responses
    h1 = [2, 2.5, 3];
    h2 = [10, 2, 1.5];
    
    % Define modulation types
    modTypes = {'BPSK', 'QPSK', '16QAM', '64QAM'};
    
    % Iterate through each modulation type
    for j = 1:length(modTypes)
        modType = modTypes{j};
        
        % Generate PN Sequence
        [InPutStream, Scrambler, descrambler] = generate_pn_sequence(PNSeqType, SamplesPerFrameNum);
        ScrambledOut = Scrambler(InPutStream);

        % Select modulation scheme
        [modulator, demodulator] = select_modulation_scheme(modType);
        
        % Modulate Signal
        ModulatedSignal = modulator(ScrambledOut);
        scatterplot(ModulatedSignal);

        % IFFT Operation
        ModulatedSignalIFFT = ifft(ModulatedSignal);

        % Adding Cyclic Prefix
        txout = ModulatedSignalIFFT(:);
        txcp = txout(end-CPLength+1:end);
        txout_cp = [txcp; txout];

        % Apply channel effect using convolution
        channelOutput1 = conv(txout_cp, h1, 'same');
        channelOutput2 = conv(txout_cp, h2, 'same');
        
        % Pass through Channel (AWGN)
        ModulatedSignalAfterChannel1 = awgn(channelOutput1, SNR);
        ModulatedSignalAfterChannel2 = awgn(channelOutput2, SNR);

        % Remove Cyclic Prefix
        rxout1 = ModulatedSignalAfterChannel1(CPLength+1:end);
        rxout2 = ModulatedSignalAfterChannel2(CPLength+1:end);

        % FFT Operation
        fftSignal1 = fft(rxout1);
        fftSignal2 = fft(rxout2);
        
        %plot
        scatterplot(fftSignal1);
        scatterplot(fftSignal2);
        % Demodulate Signal
        DemodulatedSignal1 = demodulator(fftSignal1);
        DemodulatedSignal2 = demodulator(fftSignal2);

        % Descramble Received Signal
        DeScrambledReceived1 = descrambler(DemodulatedSignal1);
        DeScrambledReceived2 = descrambler(DemodulatedSignal2);

         % Calculate Bit Error Rate
        [number1, ratio1] = calculate_ber_from_signals(InPutStream, DeScrambledReceived1);
        [number2, ratio2] = calculate_ber_from_signals(InPutStream, DeScrambledReceived2);

        % Display BER for each channel
        fprintf('Modulation: %s | Channel h1 BER: %f | Channel h2 BER: %f\n', modType, ratio1, ratio2);
    end
end



function [snrValues, berResults, modTypes] = calculate_ber(PNSeqType, SamplesPerFrameNum,action)
    % Define modulation types and SNR values
    modTypes = {'BPSK', 'QPSK', '16QAM', '64QAM'};
    snrValues = 0:1:90; % Use smaller increments for smoother curves

    % Initialize results storage
    berResults = cell(length(modTypes), 1);
    switch action
        case 1
            % Run the modulation simulation for each modulation type
            for j = 1:length(modTypes)
                berValues = zeros(size(snrValues));
                modType = modTypes{j};
                for i = 1:length(snrValues)
                    [number, ratio] = run_modulation_simulation_no_impairments(modType, PNSeqType, SamplesPerFrameNum, snrValues(i));
                    berValues(i) = ratio;
                end
                berResults{j} = berValues; % Store results
            end


        case 2
            % Run the modulation simulation for each modulation type
            for j = 1:length(modTypes)
                berValues = zeros(size(snrValues));
                modType = modTypes{j};
                for i = 1:length(snrValues)
                    [number, ratio] = run_modulation_simulation_impairments(modType, PNSeqType, SamplesPerFrameNum, snrValues(i));
                    berValues(i) = ratio;
                end
                berResults{j} = berValues; % Store results
            end

        case 3 
            % Run the modulation simulation for each modulation type
            for j = 1:length(modTypes)
                berValues = zeros(size(snrValues));
                modType = modTypes{j};
                for i = 1:length(snrValues)
                    [number, ratio] = run_modulation_simulation_conv(modType, PNSeqType, SamplesPerFrameNum, snrValues(i));
                    berValues(i) = ratio;
                end
                berResults{j} = berValues; % Store results
            end

    end
end


function [number, ratio] = run_modulation_simulation_no_impairments(modType, PNSeqType, SamplesPerFrame, SNR)
    % Generate PN Sequence
    [InPutStream, Scrambler, descrambler] = generate_pn_sequence(PNSeqType, SamplesPerFrame);

    ScrambledOut = Scrambler(InPutStream);
    
    % Modulate Signal
    [modulator, demodulator] = select_modulation_scheme(modType);
    ModulatedSignal = modulator(ScrambledOut);

    % Pass through Channel (AWGN)
    ModulatedSignalifft = ifft(ModulatedSignal);
    ModulatedSignalAfterChannel = awgn(ModulatedSignalifft, SNR);

    % Demodulate Signal
    fftSignal = fft(ModulatedSignalAfterChannel);
    DemodulatedSignal = demodulator(fftSignal);

    % Descramble Received Signal
    DeScrambledReceived = descrambler(DemodulatedSignal);
    
    % Calculate Bit Error Rate
    [number, ratio] = calculate_ber_from_signals(InPutStream, DeScrambledReceived);
end


function [number, ratio] = run_modulation_simulation_impairments(modType, PNSeqType, SamplesPerFrame, SNR)
    % Generate PN Sequence
    [InPutStream, Scrambler, descrambler] = generate_pn_sequence(PNSeqType, SamplesPerFrame);

    ScrambledOut = Scrambler(InPutStream);
    
    % Modulate Signal
    [modulator, demodulator] = select_modulation_scheme(modType);
    ModulatedSignal = modulator(ScrambledOut);

    % Pass through Channel (AWGN)
    ModulatedSignalifft = ifft(ModulatedSignal);
    ModulatedSignalAfterChannel = awgn(ModulatedSignalifft, SNR);

    % Demodulate Signal
    fftSignal = fft(ModulatedSignalAfterChannel);
    DemodulatedSignal = demodulator(fftSignal);

    % Descramble Received Signal
    DeScrambledReceived = descrambler(DemodulatedSignal);
    
    % Calculate Bit Error Rate
    [number, ratio] = calculate_ber_from_signals(InPutStream, DeScrambledReceived);
end


function [number, ratio] = run_modulation_simulation_conv(modType, PNSeqType, SamplesPerFrame, SNR)
    % Generate PN Sequence
    [InPutStream, Scrambler, descrambler] = generate_pn_sequence(PNSeqType, SamplesPerFrame);


    % Convolutional Encoding
    convencoder = comm.ConvolutionalEncoder;
    codeword = convencoder(InPutStream);
    ScrambledOut = Scrambler(codeword);

    % Modulate Signal
    [modulator, demodulator] = select_modulation_scheme(modType);
    ModulatedSignal = modulator(ScrambledOut);

    % Pass through Channel (AWGN)
    ModulatedSignalifft = ifft(ModulatedSignal);
    ModulatedSignalAfterChannel = awgn(ModulatedSignalifft, SNR);

    % Demodulate Signal
    fftSignal = fft(ModulatedSignalAfterChannel);
    DemodulatedSignal = demodulator(fftSignal);

    % Descramble Received Signal
    DeScrambledReceived = descrambler(DemodulatedSignal);

    % Viterbi Decoder
    viterbidecoder = comm.ViterbiDecoder;
    decmsg = viterbidecoder(DeScrambledReceived);

    % Calculate Bit Error Rate
    [number, ratio] = calculate_ber_from_signals(InPutStream, decmsg);
end

function [InPutStream, Scrambler, descrambler] = generate_pn_sequence(PNSeqType, SamplesPerFrame)
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
end

function [modulator, demodulator] = select_modulation_scheme(modType)
    % Select modulation and demodulation functions based on modulation type
    switch modType
        case 'BPSK'
            M = 2; % BPSK
            modulator = @(x) pskmod(x, M);
            demodulator = @(x) pskdemod(x, M);
        case 'QPSK'
            M = 4; % QPSK (which is 4-PSK)
            modulator = @(x) pskmod(x, M, pi/4, 'InputType', 'bit');
            demodulator = @(x) pskdemod(x, M, pi/4, 'OutputType', 'bit');
        case '16QAM'
            M = 16; % 16-QAM
            modulator = @(x) qammod(x, M, 'InputType', 'bit', 'UnitAveragePower', true);
            demodulator = @(x) qamdemod(x, M, 'OutputType', 'bit', 'UnitAveragePower', true);
        case '64QAM'
            M = 64; % 64-QAM
            modulator = @(x) qammod(x, M, 'InputType', 'bit', 'UnitAveragePower', true);
            demodulator = @(x) qamdemod(x, M, 'OutputType', 'bit', 'UnitAveragePower', true);
        otherwise
            error('Invalid modulation type. Please use "BPSK", "QPSK", "16QAM", or "64QAM".');
    end
end

function [number, ratio] = calculate_ber_from_signals(InPutStream, DeScrambledReceived)
    % Calculate bit error rate
    [number, ratio] = biterr(InPutStream, DeScrambledReceived);
end

function plot_ber(snrValues, berResults, modTypes)
    % Plot BER vs. SNR for Different Modulation Schemes
    figure;
    hold on;
    for j = 1:length(modTypes)
        plot(snrValues, berResults{j}, 'DisplayName', modTypes{j});
    end
    title('BER vs. SNR for Different Modulation Schemes');
    xlabel('SNR (dB)');
    ylabel('Bit Error Rate (BER)');
    legend show;
    grid on;
    hold off;
end