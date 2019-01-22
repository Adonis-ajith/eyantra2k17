############################################################################
#	    								   #
#			Musical Note Identification		           #
#   									   #
############################################################################

import numpy as np
import wave
import struct
import warnings

#warnings.filterwarnings('ignore')                      # to ignore warnings

# notes and their frequencies
notes_dictionary = {1047:'C6', 2093:'C7', 4186:'C8',\
                    1175:'D6', 2349:'D7', 4699:'D8',\
                    1319:'E6', 2637:'E7', 5274:'E8',\
                    1397:'F6', 2794:'F7', 5588:'F8',\
                    1568:'G6', 3136:'G7', 6272:'G8',\
                    1760:'A6', 3520:'A7', 7040:'A8',\
                    1976:'B6', 3951:'B7', 7902:'B8'}


sampling_freq = 44100	                                # Sampling frequency of audio signal
window = 0.05                                           # window size = 0.05 seconds
window_length = int(sampling_freq*window)               # 44100 * 0.05 = 2205 , converting to int type to avoid warning
    
def rms(array,window_length):                           # function to find RMS using convolution
    
    array2 = np.power(array,2)                          # squaring of elements
    w = np.ones(window_length)/float(window_length)     # this gives array with all elements = 1/window_size
    
    return np.sqrt(np.convolve(array2,w,'valid'))       # sqrt of convolution of array2 and array w ,'valid' = removes boundary error and 
                                                        # returns array of length max(N,M)-min(N,M)+1    window_size = N , length of array = M
    

def get_freq(note):                                     # function to get frequency of a note 

    fft_out = np.fft.rfft(note)                         # real fft of the note
    i_max = np.argmax(np.abs(fft_out))                  # to find the index i_max at which the maximum peak occurs
    frequency=abs(i_max * sampling_freq)//len(note)     # frequency = i_max*Fs/L
    
    for i in range(-5,6):                               # to find the closest frequency with error +-5
        if (frequency+i in notes_dictionary)== True :
            return notes_dictionary[frequency+i]
       

def play(sound_file):                                   # play function

    file_length = sound_file.getnframes()
    sound = np.zeros(file_length)
    for i in range(file_length):
        data = sound_file.readframes(1)
        data = struct.unpack("<h",data)
        sound[i] = int(data[0])
    sound = np.divide(sound,float(2**15))
    
    sound_rms = rms(sound,window_length)                # find rms of the sampled signal with window = 0.05 Seconds 
                                                        # using Convolution method
    a=b=k=0                                             # k = loop variable , a and b = indices of the note
    Identified_Notes = list()

    while (k < len(sound_rms)):                         # detecting note by checking rms value > 0.7(threshold) 
                                                        # and passing sound[b:a] to get_freq function
        while (sound_rms[k] >= 0.7) :                   # when rms value >= threshold ,indicates 
                                                        # note and taking its last index as 'a'
            k+=1
            if k >= len(sound_rms):                     
                break                                   
            a=k
            
        Identified_Notes.append(get_freq(sound[b:a]))   # passing the range [b:a] of sound array 
                                                        # to get_freq and appending to the list
                                                        
        while (sound_rms[k] < 0.7) :                    # when rms value < threshold ,indicates silence 
                                                        # silence and taking its index as 'b'
            k+=1                                       
            if k >= len(sound_rms):                     
                break                                  
            b=k      

        k+=1
        if k >= len(sound_rms):                         
            break
        
    return Identified_Notes                             # returning the notes 

############################## Read Audio File #############################

if __name__ == "__main__" :

    #code for checking output for single audio file

    sound_file = wave.open('Audio_files/Audio_1.wav', 'r')
    Identified_Notes = play(sound_file)
    print ("Single Audio File Notes = \n\n", Identified_Notes,'\n')

    #code for checking output for all images

    Identified_Notes_list = []
    print ("\n\nAll Audio File Notes = \n")
    for file_number in range(1,6):
        file_name = "Audio_files/Audio_"+str(file_number)+".wav"
        sound_file = wave.open(file_name)
        Identified_Notes = play(sound_file)
        Identified_Notes_list.append(Identified_Notes)      
        print ('Audio File',file_number,' : ',Identified_Notes_list[file_number-1])
    
